import "dotenv/config";
import fs from "fs";
import path from "path";
import net from "node:net";
import crypto from "node:crypto";
import express from "express";
import rateLimit from "express-rate-limit";

import { pool, migrate } from "./db.js";
import { signToken, authRequired } from "./auth.js";
import {
  normalizeLogin,
  normalizeEmail,
  isValidLogin,
  isValidPassword,
  isValidEmail
} from "./validators.js";
import {
  accountFilePath,
  accountFileExists,
  createAccountFile,
  deleteAccountFile,
  changeAccountFilePassword
} from "./accountFile.js";
import {
  getMercadoPagoConfig,
  parseMercadoPagoWebhook,
  verifyMercadoPagoWebhookSignature,
  extractOrderPix,
  orderIsPaid,
  orderMatchesDonateOrder
} from "./mercadoPago.js";
import { sendPasswordResetEmail, mailerConfigured } from "./mailer.js";
import { generateResetToken, hashToken, resetExpiry, isTokenValidRow } from "./resetTokens.js";
import { decodeGuildmarkBmp, guildmarkFilename } from "./guildmark.js";

const app = express();
app.set("trust proxy", true); // atras do Cloudflare
app.use(express.json({ limit: "16kb" }));

function isDuplicateKeyError(err) {
  return err?.code === "ER_DUP_ENTRY" || err?.errno === 1062;
}

async function findAccountByEmail(email, exceptLogin = null) {
  const params = [email];
  let sql = "SELECT username FROM accounts WHERE email IS NOT NULL AND LOWER(TRIM(email))=LOWER(?)";
  if (exceptLogin) {
    sql += " AND UPPER(username)<>UPPER(?)";
    params.push(exceptLogin);
  }
  sql += " LIMIT 1";
  const [rows] = await pool.query(sql, params);
  return rows[0] || null;
}

function guildmarkPublicUrl(req, filename) {
  const encoded = encodeURIComponent(filename);
  return `${req.protocol}://${req.get("host")}/guildmarks/${encoded}`;
}

function normalizeGuildmarkName(mark, guild) {
  const value = String(mark || "").trim();
  if (/^b01\d{6}\.bmp$/i.test(value)) return value;
  return guildmarkFilename(guild);
}

async function getLeaderGuilds(login, req) {
  const [rows] = await pool.query(
    `SELECT DISTINCT g.guild, g.name, g.mark, g.kingdom, g.fame, c.nick AS leader
       FROM accounts a
       JOIN characteres c ON c.account_id = a.id
       JOIN guilds g ON g.guild = c.guild_id
      WHERE UPPER(a.username)=UPPER(?)
        AND COALESCE(c.guild_id, 0) > 0
        AND c.guildlevel = 9
      ORDER BY g.guild ASC`,
    [login]
  );

  return rows.map((g) => {
    const filename = normalizeGuildmarkName(g.mark, g.guild);
    const filePath = path.join(GUILDMARK_DIR, filename);
    const hasMark = fs.existsSync(filePath);
    return {
      id: Number(g.guild),
      name: g.name,
      kingdom: Number(g.kingdom || 0),
      fame: Number(g.fame || 0),
      leader: g.leader || "",
      mark: filename,
      hasMark,
      markUrl: hasMark ? guildmarkPublicUrl(req, filename) : null
    };
  });
}

async function findLeaderGuild(login, guild) {
  const [rows] = await pool.query(
    `SELECT g.guild, g.name, g.mark, g.kingdom, g.fame, c.nick AS leader
       FROM accounts a
       JOIN characteres c ON c.account_id = a.id
       JOIN guilds g ON g.guild = c.guild_id
      WHERE UPPER(a.username)=UPPER(?)
        AND g.guild = ?
        AND c.guildlevel = 9
      LIMIT 1`,
    [login, guild]
  );
  return rows[0] || null;
}

// ---- CORS (libera so as origens do WYD Kingdom) ----
const ALLOWED_ORIGINS = (process.env.ALLOWED_ORIGINS || "")
  .split(",")
  .map((s) => s.trim())
  .filter(Boolean);

app.use((req, res, next) => {
  const origin = req.headers.origin;
  if (origin && ALLOWED_ORIGINS.includes(origin)) {
    res.setHeader("Access-Control-Allow-Origin", origin);
    res.setHeader("Vary", "Origin");
  }
  res.setHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  res.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
  res.setHeader("Access-Control-Max-Age", "86400");
  if (req.method === "OPTIONS") return res.status(204).end();
  next();
});

// IP real do jogador (vem no header do Cloudflare)
const realIp = (req) => req.headers["cf-connecting-ip"] || req.ip;

const loginLimiter = rateLimit({
  windowMs: 5 * 60 * 1000,
  max: 20,
  standardHeaders: true,
  legacyHeaders: false,
  keyGenerator: realIp,
  message: { ok: false, message: "Muitas tentativas. Tente novamente em alguns minutos." }
});

// Recuperacao de senha: limite mais apertado (evita spam/mailbomb).
const recoverLimiter = rateLimit({
  windowMs: 15 * 60 * 1000,
  max: 5,
  standardHeaders: true,
  legacyHeaders: false,
  keyGenerator: realIp,
  message: { ok: false, message: "Muitas tentativas. Tente novamente mais tarde." }
});

const guildmarkLimiter = rateLimit({
  windowMs: 60 * 60 * 1000,
  max: 12,
  standardHeaders: true,
  legacyHeaders: false,
  keyGenerator: realIp,
  message: { ok: false, message: "Muitos envios de guildmark. Tente novamente mais tarde." }
});

// ----------------------------------------------------------------------------
// Saude / status dos canais (lido pela lista de servidores do cliente)
// ----------------------------------------------------------------------------
app.get("/health", (_req, res) => res.type("text/plain").send("OK"));

app.get(["/serv00.htm", "/serv00.htm/"], (_req, res) => {
  const statusFile = process.env.STATUS_FILE;
  if (statusFile && fs.existsSync(statusFile)) {
    res.type("text/plain").send(fs.readFileSync(statusFile));
    return;
  }
  // Padrao: 2 canais online, resto offline (igual ao script antigo).
  const lines = Array.from({ length: 10 }, (_, i) => (i < 2 ? "0" : "-1"));
  res.type("text/plain").send(lines.join("\r\n") + "\r\n");
});

// ----------------------------------------------------------------------------
// Patches do launcher
// ----------------------------------------------------------------------------
const PATCH_DIR = path.resolve(
  process.env.PATCH_DIR || path.join(process.cwd(), "patch")
);
const GUILDMARK_DIR = path.resolve(
  process.env.GUILDMARK_DIR || path.join(PATCH_DIR, "guildmarks")
);

app.get("/patch/manifest.json", (_req, res) => {
  const manifestPath = path.join(PATCH_DIR, "manifest.json");
  if (!fs.existsSync(manifestPath))
    return res.status(404).json({ ok: false, message: "Manifesto de patch ainda nao publicado." });

  res.setHeader("Cache-Control", "no-store, no-cache, must-revalidate, proxy-revalidate");
  res.setHeader("Pragma", "no-cache");
  res.setHeader("Expires", "0");
  return res.sendFile(manifestPath);
});

app.use("/patch", express.static(PATCH_DIR, {
  index: false,
  dotfiles: "deny",
  fallthrough: true,
  setHeaders: (res, filePath) => {
    if (path.basename(filePath).toLowerCase() === "manifest.json") {
      res.setHeader("Cache-Control", "no-store");
      return;
    }
    res.setHeader("Cache-Control", "public, max-age=31536000, immutable");
  }
}));

app.use("/guildmarks", express.static(GUILDMARK_DIR, {
  index: false,
  dotfiles: "deny",
  fallthrough: true,
  setHeaders: (res) => {
    res.setHeader("Cache-Control", "public, max-age=60");
  }
}));

// ----------------------------------------------------------------------------
// Status do servidor para o site (online agora + status + versao + rates)
// ----------------------------------------------------------------------------
const GAME_HOST = process.env.GAME_HOST || "127.0.0.1";
const GAME_PORT = Number(process.env.GAME_PORT || 8281);

// Confere se o TMSrv esta de pe tentando conectar na porta do jogo.
function checkGameUp(timeoutMs = 1500) {
  return new Promise((resolve) => {
    const socket = new net.Socket();
    let done = false;
    const finish = (ok) => {
      if (done) return;
      done = true;
      socket.destroy();
      resolve(ok);
    };
    socket.setTimeout(timeoutMs);
    socket.once("connect", () => finish(true));
    socket.once("timeout", () => finish(false));
    socket.once("error", () => finish(false));
    socket.connect(GAME_PORT, GAME_HOST);
  });
}

// Le a versao do patch publicado (manifest.json) para nunca ficar desatualizada.
function readPatchVersion() {
  try {
    const manifestPath = path.join(PATCH_DIR, "manifest.json");
    // strip BOM: alguns manifestos publicados vêm com BOM UTF-8 e quebram o JSON.parse
    const raw = JSON.parse(fs.readFileSync(manifestPath, "utf8").replace(/^\uFEFF/, ""));
    if (raw && raw.version) return String(raw.version);
  } catch {}
  return process.env.SITE_VERSION || "—";
}

function renderGuildmarkPage() {
  return `<!doctype html>
<html lang="pt-BR">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Guildmark - WYD Kingdom</title>
  <style>
    :root {
      color-scheme: dark;
      --bg: #060913;
      --panel: #111724;
      --panel-2: #151d2b;
      --line: #2a3345;
      --text: #f6f7fb;
      --muted: #a7afbf;
      --accent: #a875ff;
      --accent-2: #50d6ff;
      --ok: #5ee4a6;
      --bad: #ff6b81;
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      min-height: 100vh;
      font-family: Inter, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
      background:
        radial-gradient(circle at 25% 5%, rgba(126, 76, 255, .24), transparent 36rem),
        radial-gradient(circle at 75% 18%, rgba(58, 198, 255, .16), transparent 34rem),
        var(--bg);
      color: var(--text);
    }
    a { color: inherit; text-decoration: none; }
    .shell { width: min(980px, calc(100% - 32px)); margin: 0 auto; padding: 42px 0; }
    .top { display: flex; align-items: center; justify-content: space-between; gap: 16px; margin-bottom: 22px; }
    .brand { display: flex; align-items: center; gap: 14px; min-width: 0; }
    .crest {
      width: 48px; height: 48px; border: 1px solid rgba(255,255,255,.18); border-radius: 8px;
      display: grid; place-items: center; color: var(--accent); font-weight: 900; background: rgba(255,255,255,.04);
    }
    h1 { margin: 0; font-size: clamp(26px, 4vw, 42px); letter-spacing: 0; }
    .subtitle { margin: 6px 0 0; color: var(--muted); font-size: 15px; }
    .back {
      display: inline-flex; align-items: center; justify-content: center; min-height: 42px;
      border: 1px solid rgba(255,255,255,.25); border-radius: 8px; padding: 0 16px; color: var(--text);
      background: rgba(255,255,255,.03); font-weight: 700;
    }
    .panel {
      border: 1px solid var(--line); border-radius: 8px; background: rgba(17, 23, 36, .92);
      box-shadow: 0 26px 80px rgba(0,0,0,.34); overflow: hidden;
    }
    .grid { display: grid; grid-template-columns: 310px 1fr; min-height: 520px; }
    .side { padding: 24px; background: rgba(255,255,255,.025); border-right: 1px solid var(--line); }
    .main { padding: 24px; }
    .section-title { margin: 0 0 16px; font-size: 14px; color: var(--muted); text-transform: uppercase; letter-spacing: .08em; }
    .account { padding: 14px; border: 1px solid var(--line); border-radius: 8px; background: var(--panel-2); margin-bottom: 18px; }
    .account strong { display: block; font-size: 18px; margin-top: 4px; }
    .guild-list { display: grid; gap: 10px; }
    .guild {
      width: 100%; text-align: left; border: 1px solid var(--line); border-radius: 8px; background: #0e1421;
      color: var(--text); padding: 12px; cursor: pointer;
    }
    .guild[aria-pressed="true"] { border-color: var(--accent); box-shadow: inset 0 0 0 1px var(--accent); }
    .guild-name { font-weight: 800; font-size: 15px; overflow-wrap: anywhere; }
    .guild-meta { color: var(--muted); margin-top: 4px; font-size: 13px; }
    .empty { border: 1px dashed var(--line); border-radius: 8px; padding: 16px; color: var(--muted); background: rgba(255,255,255,.025); }
    .upload { display: grid; grid-template-columns: 150px 1fr; gap: 18px; align-items: start; }
    .preview {
      width: 150px; aspect-ratio: 4 / 3; border: 1px solid var(--line); border-radius: 8px;
      display: grid; place-items: center; background: repeating-conic-gradient(#111827 0 25%, #0b101a 0 50%) 50% / 16px 16px;
      image-rendering: pixelated; overflow: hidden;
    }
    .preview img { width: 128px; height: 96px; image-rendering: pixelated; object-fit: contain; }
    .field { display: grid; gap: 8px; margin-bottom: 14px; }
    label { color: var(--muted); font-size: 13px; font-weight: 700; text-transform: uppercase; letter-spacing: .06em; }
    input[type="text"], input[type="password"], input[type="file"] {
      width: 100%; min-height: 46px; border: 1px solid var(--line); border-radius: 8px; background: #0b101a;
      color: var(--text); padding: 10px 12px; font: inherit;
    }
    .hint { color: var(--muted); font-size: 13px; line-height: 1.45; }
    .actions { display: flex; gap: 10px; flex-wrap: wrap; margin-top: 18px; }
    button.primary, button.secondary {
      min-height: 46px; border: 0; border-radius: 8px; padding: 0 18px; color: white; font-weight: 900; cursor: pointer;
    }
    button.primary { background: linear-gradient(135deg, #6d8cff, #9f5cff); }
    button.secondary { background: #222c3f; border: 1px solid var(--line); }
    button:disabled { opacity: .55; cursor: not-allowed; }
    .message { margin-top: 16px; min-height: 24px; font-weight: 700; }
    .message.ok { color: var(--ok); }
    .message.bad { color: var(--bad); }
    .login { max-width: 420px; }
    .hidden { display: none !important; }
    @media (max-width: 760px) {
      .shell { width: min(100% - 22px, 980px); padding: 24px 0; }
      .top { align-items: flex-start; }
      .grid, .upload { grid-template-columns: 1fr; }
      .side { border-right: 0; border-bottom: 1px solid var(--line); }
      .preview { width: 100%; max-width: 220px; }
    }
  </style>
</head>
<body>
  <main class="shell">
    <div class="top">
      <div class="brand">
        <div class="crest">GM</div>
        <div>
          <h1>Guildmark</h1>
          <p class="subtitle">Envio de marca da guild para aparecer no jogo.</p>
        </div>
      </div>
      <a class="back" href="/conta/">Voltar</a>
    </div>

    <section class="panel">
      <div id="loginView" class="main login">
        <p class="section-title">Entrar na conta</p>
        <div class="field">
          <label for="login">Login</label>
          <input id="login" type="text" autocomplete="username" />
        </div>
        <div class="field">
          <label for="password">Senha</label>
          <input id="password" type="password" autocomplete="current-password" />
        </div>
        <div class="actions">
          <button id="loginBtn" class="primary" type="button">Entrar</button>
        </div>
        <div id="loginMsg" class="message"></div>
      </div>

      <div id="appView" class="grid hidden">
        <aside class="side">
          <div class="account">
            <span class="hint">Conta logada</span>
            <strong id="accountName">-</strong>
          </div>
          <p class="section-title">Guilds lideradas</p>
          <div id="guildList" class="guild-list"></div>
        </aside>
        <section class="main">
          <p class="section-title">Enviar BMP</p>
          <div id="emptyState" class="empty hidden">Somente o lider da guild pode enviar guildmark. Crie ou lidere uma guild no jogo e entre novamente no site.</div>
          <div id="uploadView" class="upload hidden">
            <div class="preview" id="previewBox"><span class="hint">16x12 BMP</span></div>
            <div>
              <div class="field">
                <label for="file">Arquivo BMP</label>
                <input id="file" type="file" accept=".bmp,image/bmp" />
                <div class="hint">Formato aceito: BMP 24-bit, exatamente 16x12 pixels, ate 2KB.</div>
              </div>
              <div class="actions">
                <button id="sendBtn" class="primary" type="button" disabled>Enviar Guildmark</button>
                <button id="logoutBtn" class="secondary" type="button">Sair</button>
              </div>
              <div id="appMsg" class="message"></div>
            </div>
          </div>
        </section>
      </div>
    </section>
  </main>

  <script>
    const tokenKeys = ["wydkingdom_token", "wyd_token", "authToken", "accessToken", "jwt", "token"];
    let token = "";
    let selectedGuild = null;
    let selectedFile = null;

    const $ = (id) => document.getElementById(id);
    const setMsg = (id, text, type) => {
      const el = $(id);
      el.textContent = text || "";
      el.className = "message" + (type ? " " + type : "");
    };

    function allStoredTokens() {
      const values = [];
      const addToken = (value) => {
        if (value && String(value).split(".").length === 3 && !values.includes(value)) values.push(String(value));
      };
      for (const store of [localStorage, sessionStorage]) {
        for (const key of tokenKeys) {
          addToken(store.getItem(key));
        }
        for (let i = 0; i < store.length; i++) {
          const value = store.getItem(store.key(i));
          addToken(value);
          try {
            const parsed = JSON.parse(value);
            if (parsed && typeof parsed === "object") {
              addToken(parsed.token);
              addToken(parsed.authToken);
              addToken(parsed.accessToken);
              addToken(parsed.jwt);
            }
          } catch {}
        }
      }
      return values;
    }

    async function api(path, options = {}, useToken = token) {
      const headers = Object.assign({ "Content-Type": "application/json" }, options.headers || {});
      if (useToken) headers.Authorization = "Bearer " + useToken;
      const res = await fetch(path, Object.assign({}, options, { headers }));
      const data = await res.json().catch(() => ({}));
      if (!res.ok || data.ok === false) throw new Error(data.message || "Falha na requisicao.");
      return data;
    }

    async function findSession() {
      for (const candidate of allStoredTokens()) {
        try {
          const me = await api("/api/account/me", {}, candidate);
          token = candidate;
          localStorage.setItem("wydkingdom_token", token);
          return me.user;
        } catch {}
      }
      return null;
    }

    async function loadGuilds(user) {
      $("loginView").classList.add("hidden");
      $("appView").classList.remove("hidden");
      $("accountName").textContent = user.login;
      setMsg("appMsg", "", "");

      const data = await api("/api/account/guildmark");
      const guilds = data.guilds || [];
      const list = $("guildList");
      list.innerHTML = "";

      if (!guilds.length) {
        $("emptyState").classList.remove("hidden");
        $("uploadView").classList.add("hidden");
        list.innerHTML = '<div class="empty">Nenhuma guild liderada nesta conta.</div>';
        return;
      }

      $("emptyState").classList.add("hidden");
      $("uploadView").classList.remove("hidden");
      selectedGuild = guilds[0];

      for (const guild of guilds) {
        const btn = document.createElement("button");
        btn.type = "button";
        btn.className = "guild";
        btn.setAttribute("aria-pressed", String(guild.id === selectedGuild.id));
        btn.innerHTML = '<div class="guild-name">' + guild.name + '</div><div class="guild-meta">ID ' + guild.id + ' - Lider ' + (guild.leader || "-") + '</div>';
        btn.onclick = () => {
          selectedGuild = guild;
          document.querySelectorAll(".guild").forEach((b) => b.setAttribute("aria-pressed", "false"));
          btn.setAttribute("aria-pressed", "true");
          setMsg("appMsg", "Guild selecionada: " + guild.name, "ok");
        };
        list.appendChild(btn);
      }
    }

    function fileToBase64(file) {
      return new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.onload = () => resolve(String(reader.result).split(",")[1] || "");
        reader.onerror = () => reject(new Error("Nao foi possivel ler o arquivo."));
        reader.readAsDataURL(file);
      });
    }

    $("loginBtn").onclick = async () => {
      try {
        setMsg("loginMsg", "Entrando...", "");
        const data = await api("/api/login", {
          method: "POST",
          body: JSON.stringify({ login: $("login").value, password: $("password").value })
        }, "");
        token = data.token;
        localStorage.setItem("wydkingdom_token", token);
        localStorage.setItem("token", token);
        await loadGuilds(data.user);
      } catch (err) {
        setMsg("loginMsg", err.message, "bad");
      }
    };

    $("file").onchange = () => {
      selectedFile = $("file").files[0] || null;
      $("sendBtn").disabled = !selectedFile || !selectedGuild;
      $("previewBox").innerHTML = selectedFile ? '<img alt="Preview" src="' + URL.createObjectURL(selectedFile) + '">' : '<span class="hint">16x12 BMP</span>';
      setMsg("appMsg", selectedFile ? selectedFile.name : "", selectedFile ? "ok" : "");
    };

    $("sendBtn").onclick = async () => {
      if (!selectedGuild || !selectedFile) return;
      try {
        $("sendBtn").disabled = true;
        setMsg("appMsg", "Enviando...", "");
        const imageBase64 = await fileToBase64(selectedFile);
        const data = await api("/api/account/guildmark", {
          method: "POST",
          body: JSON.stringify({ guild: selectedGuild.id, imageBase64 })
        });
        setMsg("appMsg", data.message || "Guildmark enviada.", "ok");
      } catch (err) {
        setMsg("appMsg", err.message, "bad");
      } finally {
        $("sendBtn").disabled = !selectedFile || !selectedGuild;
      }
    };

    $("logoutBtn").onclick = () => {
      for (const key of tokenKeys) {
        localStorage.removeItem(key);
        sessionStorage.removeItem(key);
      }
      location.href = "/conta/";
    };

    findSession()
      .then((user) => user ? loadGuilds(user) : null)
      .catch((err) => setMsg("loginMsg", err.message, "bad"));
  </script>
</body>
</html>`;
}

app.get(["/conta/guildmark", "/conta/guildmaker", "/guildmark", "/guildmaker"], (_req, res) => {
  res.setHeader("Cache-Control", "no-store");
  res.type("html").send(renderGuildmarkPage());
});

app.get("/api/status", async (_req, res) => {
  const up = await checkGameUp();

  // Contagem real gravada pelo TMSrv em serverconfig.online_count (a cada 10s).
  // So vale se o servidor estiver de pe; se caiu, mostra 0.
  let online = 0;
  if (up) {
    try {
      const [rows] = await pool.query(
        "SELECT online_count AS n FROM serverconfig WHERE id = 1"
      );
      online = Number(rows?.[0]?.n || 0);
    } catch {
      online = 0;
    }
  }

  res.setHeader("Cache-Control", "no-store");
  res.json({
    ok: true,
    status: up ? "online" : "offline",
    online,
    version: readPatchVersion(),
    rates: {
      exp: process.env.RATE_EXP || "500x",
      drop: process.env.RATE_DROP || "300x",
      gold: process.env.RATE_GOLD || "100x"
    }
  });
});

// ----------------------------------------------------------------------------
// Downloads publicos (instalador oficial do cliente)
// ----------------------------------------------------------------------------
const DOWNLOADS_DIR = path.resolve(
  process.env.DOWNLOADS_DIR || path.join(process.cwd(), "downloads")
);

app.use("/downloads", express.static(DOWNLOADS_DIR, {
  index: false,
  dotfiles: "deny",
  fallthrough: true,
  setHeaders: (res, filePath) => {
    // Forca "salvar como" no navegador e cache curto (instalador pode ser trocado)
    res.setHeader("Content-Disposition", `attachment; filename="${path.basename(filePath)}"`);
    res.setHeader("Cache-Control", "public, max-age=300");
  }
}));

// ----------------------------------------------------------------------------
// Cadastro (migrado do webstatus PowerShell)
// ----------------------------------------------------------------------------
app.post("/api/register", async (req, res) => {
  try {
    const login = normalizeLogin(req.body.login);
    const password = String(req.body.password || "").trim();
    const email = normalizeEmail(req.body.email);

    if (!isValidLogin(login))
      return res.status(400).json({ ok: false, message: "Login invalido (4 a 12, comeca com letra, apenas letras e numeros)." });
    if (!isValidPassword(password))
      return res.status(400).json({ ok: false, message: "Senha invalida (4 a 10, apenas letras e numeros)." });
    if (!isValidEmail(email))
      return res.status(400).json({ ok: false, message: "Informe um e-mail valido (necessario para recuperar a senha)." });

    const [rows] = await pool.query(
      "SELECT COUNT(*) AS c FROM accounts WHERE UPPER(username)=UPPER(?)",
      [login]
    );
    if (rows[0].c > 0 || accountFileExists(login))
      return res.status(400).json({ ok: false, message: "Esse login ja existe." });

    if (await findAccountByEmail(email))
      return res.status(409).json({ ok: false, message: "Esse e-mail ja esta vinculado a outra conta." });

    createAccountFile(login, password);
    try {
      await pool.query(
        "INSERT INTO accounts (user_id, username, password, donate, online, numerica, divina, Pix, email) VALUES (1, ?, ?, 0, 0, 0, 0, '0', ?)",
        [login, password, email]
      );
    } catch (err) {
      deleteAccountFile(login); // rollback do arquivo se o banco falhar
      if (isDuplicateKeyError(err))
        return res.status(409).json({ ok: false, message: "Esse e-mail ja esta vinculado a outra conta." });
      throw err;
    }

    return res.json({ ok: true, login });
  } catch (err) {
    return res.status(400).json({ ok: false, message: err.message || "Erro ao criar conta." });
  }
});

// ----------------------------------------------------------------------------
// Login
// ----------------------------------------------------------------------------
app.post("/api/login", loginLimiter, async (req, res) => {
  try {
    const login = normalizeLogin(req.body.login);
    const password = String(req.body.password || "");
    if (!login || !password)
      return res.status(400).json({ ok: false, message: "Informe login e senha." });

    const [rows] = await pool.query(
      "SELECT username, password, donate, email FROM accounts WHERE UPPER(username)=UPPER(?) LIMIT 1",
      [login]
    );
    if (!rows.length || String(rows[0].password) !== password)
      return res.status(401).json({ ok: false, message: "Login ou senha incorretos." });

    const acc = rows[0];
    const token = signToken(acc.username);
    return res.json({
      ok: true,
      token,
      user: { login: acc.username, email: acc.email || "", donate: acc.donate || 0 }
    });
  } catch (err) {
    return res.status(500).json({ ok: false, message: "Erro no login. Tente novamente." });
  }
});

// ----------------------------------------------------------------------------
// Recuperacao de senha — ETAPA 1: solicitar.
// Recebe login OU e-mail. Gera um token temporario (30 min), salva so o HASH
// no banco, invalida tokens anteriores e envia um link/codigo por e-mail.
// NUNCA envia a senha atual. Resposta sempre generica (nao revela se existe).
// ----------------------------------------------------------------------------
const SITE_URL = (process.env.SITE_URL || "https://wydkingdom.com.br").replace(/\/+$/, "");
const RESET_EXPIRES_MIN = Number(process.env.RESET_EXPIRES_MIN || "30") || 30;

app.post("/api/account/recover", recoverLimiter, async (req, res) => {
  const generic = {
    ok: true,
    message: "Se os dados estiverem corretos, enviaremos instrucoes para o e-mail cadastrado."
  };

  if (!mailerConfigured())
    return res.status(503).json({ ok: false, message: "Recuperacao por e-mail ainda nao esta disponivel." });

  const identifier = String(req.body.login || req.body.email || "").trim();
  if (!identifier) return res.json(generic);

  try {
    let acc = null;
    if (identifier.includes("@")) {
      const email = normalizeEmail(identifier);
      if (isValidEmail(email)) {
        const [rows] = await pool.query(
          "SELECT username, email FROM accounts WHERE email IS NOT NULL AND LOWER(TRIM(email))=LOWER(?) LIMIT 1",
          [email]
        );
        acc = rows[0] || null;
      }
    } else {
      const login = normalizeLogin(identifier);
      if (isValidLogin(login)) {
        const [rows] = await pool.query(
          "SELECT username, email FROM accounts WHERE UPPER(username)=UPPER(?) LIMIT 1",
          [login]
        );
        acc = rows[0] || null;
      }
    }

    if (acc && acc.email && isValidEmail(acc.email)) {
      // invalida tokens pendentes anteriores da mesma conta
      await pool.query("UPDATE password_resets SET used=1 WHERE login=? AND used=0", [acc.username]);
      const { token, hash } = generateResetToken();
      await pool.query(
        "INSERT INTO password_resets (login, token_hash, expires_at, used) VALUES (?, ?, ?, 0)",
        [acc.username, hash, resetExpiry(RESET_EXPIRES_MIN)]
      );
      const resetUrl = `${SITE_URL}/recuperar?token=${token}`;
      await sendPasswordResetEmail(acc.email, acc.username, resetUrl, token, RESET_EXPIRES_MIN);
    }
  } catch (err) {
    console.error("recover erro:", err.message);
  }

  return res.json(generic);
});

// ----------------------------------------------------------------------------
// Recuperacao de senha — ETAPA 2: redefinir.
// Recebe token + nova senha. Valida token (existe, nao usado, nao expirado),
// exige a conta OFFLINE e grava a nova senha pelo MESMO caminho do "trocar
// senha" do painel (arquivo binario + banco). Token vira "usado" e os demais
// tokens da conta sao invalidados.
// ----------------------------------------------------------------------------
app.post("/api/account/reset-password", recoverLimiter, async (req, res) => {
  const token = String(req.body.token || "").trim();
  const next = String(req.body.password || "").trim();
  const confirm = String(req.body.confirm ?? req.body.password ?? "").trim();

  if (!token) return res.status(400).json({ ok: false, message: "Link/codigo invalido." });
  if (!isValidPassword(next))
    return res.status(400).json({ ok: false, message: "Nova senha invalida (4 a 10, apenas letras e numeros)." });
  if (next !== confirm)
    return res.status(400).json({ ok: false, message: "As senhas nao conferem." });

  const conn = await pool.getConnection();
  try {
    await conn.beginTransaction();

    const [tk] = await conn.query(
      "SELECT id, login, expires_at, used FROM password_resets WHERE token_hash=? LIMIT 1 FOR UPDATE",
      [hashToken(token)]
    );
    if (!isTokenValidRow(tk[0])) {
      await conn.rollback();
      return res.status(400).json({ ok: false, message: "Link/codigo invalido ou expirado. Solicite um novo." });
    }
    const reset = tk[0];

    const [accs] = await conn.query(
      "SELECT username, online FROM accounts WHERE UPPER(username)=UPPER(?) LIMIT 1",
      [reset.login]
    );
    if (!accs.length) {
      await conn.rollback();
      return res.status(400).json({ ok: false, message: "Conta nao encontrada." });
    }
    if (accs[0].online) {
      await conn.rollback();
      return res.status(409).json({ ok: false, message: "Saia do jogo antes de redefinir a senha (conta online)." });
    }

    // Grava no MESMO caminho do "trocar senha": arquivo binario + banco.
    changeAccountFilePassword(accs[0].username, next);
    await conn.query(
      "UPDATE accounts SET password=?, updated_at=NOW() WHERE UPPER(username)=UPPER(?)",
      [next, accs[0].username]
    );
    // marca este token como usado e invalida os demais pendentes da conta
    await conn.query("UPDATE password_resets SET used=1 WHERE id=?", [reset.id]);
    await conn.query("UPDATE password_resets SET used=1 WHERE login=? AND used=0", [reset.login]);

    await conn.commit();
    return res.json({ ok: true, message: "Senha redefinida com sucesso. Ja pode entrar no jogo." });
  } catch (err) {
    await conn.rollback();
    console.error("reset-password erro:", err.message);
    return res.status(500).json({ ok: false, message: "Erro ao redefinir a senha. Tente novamente." });
  } finally {
    conn.release();
  }
});

// ----------------------------------------------------------------------------
// Conta logada
// ----------------------------------------------------------------------------
app.get("/api/account/me", authRequired, async (req, res) => {
  const [rows] = await pool.query(
    "SELECT username, donate, email, online FROM accounts WHERE UPPER(username)=UPPER(?) LIMIT 1",
    [req.login]
  );
  if (!rows.length) return res.status(404).json({ ok: false, message: "Conta nao encontrada." });
  const a = rows[0];
  res.json({
    ok: true,
    user: { login: a.username, email: a.email || "", donate: a.donate || 0, online: !!a.online }
  });
});

// Guildmark: apenas contas com personagem lider (guildlevel 9) podem enviar.
app.get("/api/account/guildmark", authRequired, async (req, res) => {
  try {
    const guilds = await getLeaderGuilds(req.login, req);
    res.json({ ok: true, canUpload: guilds.length > 0, guilds });
  } catch (err) {
    console.error("guildmark status erro:", err.message);
    res.status(500).json({ ok: false, message: "Erro ao consultar guildmark." });
  }
});

app.post("/api/account/guildmark", authRequired, guildmarkLimiter, async (req, res) => {
  const guild = Number(req.body.guild);
  if (!Number.isInteger(guild) || guild < 1 || guild > 999999) {
    return res.status(400).json({ ok: false, message: "Guild invalida." });
  }

  try {
    const leaderGuild = await findLeaderGuild(req.login, guild);
    if (!leaderGuild) {
      return res.status(403).json({
        ok: false,
        message: "Somente o lider da guild pode enviar a guildmark."
      });
    }

    const { buffer, size } = decodeGuildmarkBmp(req.body.imageBase64);
    const filename = guildmarkFilename(guild);
    const sha256 = crypto.createHash("sha256").update(buffer).digest("hex");

    await fs.promises.mkdir(GUILDMARK_DIR, { recursive: true });
    const finalPath = path.join(GUILDMARK_DIR, filename);
    const tmpPath = path.join(GUILDMARK_DIR, `${filename}.${process.pid}.${Date.now()}.tmp`);
    await fs.promises.writeFile(tmpPath, buffer, { mode: 0o644 });
    await fs.promises.rename(tmpPath, finalPath);

    await pool.query(
      "UPDATE guilds SET mark=?, updated_at=NOW() WHERE guild=?",
      [filename, guild]
    );
    await pool.query(
      "INSERT INTO guildmark_uploads (login, guild, filename, size, sha256, ip) VALUES (?, ?, ?, ?, ?, ?)",
      [req.login, guild, filename, size, sha256, String(realIp(req)).slice(0, 45)]
    );

    return res.json({
      ok: true,
      message: "Guildmark enviada com sucesso.",
      guild: {
        id: Number(leaderGuild.guild),
        name: leaderGuild.name,
        leader: leaderGuild.leader || "",
        mark: filename,
        hasMark: true,
        markUrl: guildmarkPublicUrl(req, filename)
      }
    });
  } catch (err) {
    console.error("guildmark upload erro:", err.message);
    const status = /guild|imagem|bmp|formato|compressao|pixels|2KB|incompleto/i.test(err.message)
      ? 400
      : 500;
    return res.status(status).json({
      ok: false,
      message: status === 400 ? err.message : "Erro ao enviar guildmark."
    });
  }
});

// Troca de senha (atualiza o banco E o arquivo da conta; exige estar offline)
app.post("/api/account/password", authRequired, async (req, res) => {
  try {
    const current = String(req.body.currentPassword || "");
    const next = String(req.body.newPassword || "").trim();
    if (!isValidPassword(next))
      return res.status(400).json({ ok: false, message: "Nova senha invalida (4 a 10, apenas letras e numeros)." });

    const [rows] = await pool.query(
      "SELECT username, password, online FROM accounts WHERE UPPER(username)=UPPER(?) LIMIT 1",
      [req.login]
    );
    if (!rows.length) return res.status(404).json({ ok: false, message: "Conta nao encontrada." });
    const a = rows[0];

    if (String(a.password) !== current)
      return res.status(400).json({ ok: false, message: "Senha atual incorreta." });
    if (a.online)
      return res.status(409).json({ ok: false, message: "Saia do jogo antes de trocar a senha (conta online)." });

    changeAccountFilePassword(a.username, next);
    await pool.query(
      "UPDATE accounts SET password=?, updated_at=NOW() WHERE UPPER(username)=UPPER(?)",
      [next, a.username]
    );
    res.json({ ok: true });
  } catch (err) {
    res.status(400).json({ ok: false, message: err.message || "Erro ao trocar a senha." });
  }
});

// Troca de e-mail (confirma com a senha atual)
app.post("/api/account/email", authRequired, async (req, res) => {
  try {
    const email = normalizeEmail(req.body.email);
    const password = String(req.body.password || "");
    if (!isValidEmail(email))
      return res.status(400).json({ ok: false, message: "E-mail invalido." });

    const [rows] = await pool.query(
      "SELECT username, password FROM accounts WHERE UPPER(username)=UPPER(?) LIMIT 1",
      [req.login]
    );
    if (!rows.length) return res.status(404).json({ ok: false, message: "Conta nao encontrada." });
    if (String(rows[0].password) !== password)
      return res.status(400).json({ ok: false, message: "Confirme com sua senha atual." });

    if (await findAccountByEmail(email, rows[0].username))
      return res.status(409).json({ ok: false, message: "Esse e-mail ja esta vinculado a outra conta." });

    await pool.query(
      "UPDATE accounts SET email=?, updated_at=NOW() WHERE UPPER(username)=UPPER(?)",
      [email, rows[0].username]
    );
    res.json({ ok: true, email });
  } catch (err) {
    if (isDuplicateKeyError(err))
      return res.status(409).json({ ok: false, message: "Esse e-mail ja esta vinculado a outra conta." });
    console.error("account email erro:", err.message);
    return res.status(500).json({ ok: false, message: "Erro ao trocar o e-mail. Tente novamente." });
  }
});

// ----------------------------------------------------------------------------
// Donate (Pix via Mercado Pago)
// ----------------------------------------------------------------------------
// Catalogo AUTORITATIVO (preco em R$ -> pontos de donate creditados).
// O cliente so manda o packageId; o preco/pontos saem daqui.
const DONATE_PACKAGES = {
  starter: { name: "Iniciante", amount: 10, points: 1000 },
  warrior: { name: "Guerreiro", amount: 25, points: 3000 },
  legend: { name: "Lendário", amount: 50, points: 7000 },
  immortal: { name: "Imortal", amount: 100, points: 15000 },
  evento350: {
    name: "Pacote Evento Infernal",
    amount: 350,
    points: 35000,
    items: [
      { index: 3331, eff: [0, 0, 0, 0, 0, 0] },   // 1x Passe de Evento
      { index: 482, eff: [61, 20, 0, 0, 0, 0] },  // 20x Bau do Tesouro IV (empilhado)
      { index: 4365, eff: [0, 0, 0, 0, 0, 0] }     // 1x Traje Infernal (Permanente)
    ]
  }
};

const mercadoPago = getMercadoPagoConfig();

async function ensureDonateSchema() {
  await pool.query(`
    CREATE TABLE IF NOT EXISTS donate_orders (
      id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
      login VARCHAR(12) NOT NULL,
      package_id VARCHAR(20) NOT NULL,
      amount DECIMAL(10,2) NOT NULL,
      points INT NOT NULL,
      order_id VARCHAR(64) NULL,
      payment_id VARCHAR(40) NULL,
      status VARCHAR(30) NOT NULL DEFAULT 'pending',
      credited TINYINT(1) NOT NULL DEFAULT 0,
      created_at TIMESTAMP NULL DEFAULT current_timestamp(),
      updated_at TIMESTAMP NULL DEFAULT NULL,
      PRIMARY KEY (id),
      KEY idx_order (order_id),
      KEY idx_payment (payment_id)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci
  `);
  // Migracoes compativeis com a tabela antiga (fluxo /v1/payments):
  await pool.query("ALTER TABLE donate_orders ADD COLUMN IF NOT EXISTS order_id VARCHAR(64) NULL");
  await pool.query("ALTER TABLE donate_orders MODIFY COLUMN payment_id VARCHAR(40) NULL");
  await pool.query("ALTER TABLE donate_orders MODIFY COLUMN status VARCHAR(30) NOT NULL DEFAULT 'pending'");
}

// Tabela dos tokens de redefinicao de senha (guarda so o HASH do token).
async function ensurePasswordResetSchema() {
  await pool.query(`
    CREATE TABLE IF NOT EXISTS password_resets (
      id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
      login VARCHAR(12) NOT NULL,
      token_hash CHAR(64) NOT NULL,
      expires_at DATETIME NOT NULL,
      used TINYINT(1) NOT NULL DEFAULT 0,
      created_at TIMESTAMP NULL DEFAULT current_timestamp(),
      PRIMARY KEY (id),
      KEY idx_token (token_hash),
      KEY idx_login (login)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci
  `);
}

async function ensureGuildmarkSchema() {
  await pool.query(`
    CREATE TABLE IF NOT EXISTS guildmark_uploads (
      id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
      login VARCHAR(12) NOT NULL,
      guild INT UNSIGNED NOT NULL,
      filename VARCHAR(64) NOT NULL,
      size INT UNSIGNED NOT NULL,
      sha256 CHAR(64) NOT NULL,
      ip VARCHAR(45) NULL,
      created_at TIMESTAMP NULL DEFAULT current_timestamp(),
      PRIMARY KEY (id),
      KEY idx_guild_created (guild, created_at),
      KEY idx_login_created (login, created_at)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci
  `);
}

// Pasta onde o DBSrv le entregas de item (ImportItem). Cada arquivo = 1 item.
// Linha: "CONTA INDEX EF1 VAL1 EF2 VAL2 EF3 VAL3". DBSrv entrega no guarda-carga (offline ok).
const IMPORT_ITEM_DIR = process.env.IMPORT_ITEM_DIR || "C:\\WYDKingdom\\Server\\Common\\ImportItem";
function writeImportItems(login, items) {
  fs.mkdirSync(IMPORT_ITEM_DIR, { recursive: true });
  const acc = String(login).toUpperCase();
  items.forEach((it, i) => {
    const line = `${acc} ${it.index} ${it.eff.join(" ")}\n`;
    const fname = `evt_${Date.now()}_${i}_${Math.random().toString(36).slice(2, 8)}.txt`;
    fs.writeFileSync(path.join(IMPORT_ITEM_DIR, fname), line, "ascii");
  });
}

// Credita os pontos UMA unica vez quando a Order esta paga.
// O pedido interno e localizado por external_reference (= id do donate_order).
async function creditOrderIfPaid(order) {
  const externalRef = String(order.external_reference || "");
  if (!/^\d+$/.test(externalRef)) return false;

  const pix = extractOrderPix(order);

  if (!orderIsPaid(order)) {
    await pool.query(
      "UPDATE donate_orders SET status=?, order_id=COALESCE(order_id,?), payment_id=COALESCE(payment_id,?), updated_at=NOW() WHERE id=? AND credited=0",
      [pix.status || "pending", pix.orderId || null, pix.paymentId, externalRef]
    );
    return false;
  }

  const conn = await pool.getConnection();
  try {
    await conn.beginTransaction();
    const [rows] = await conn.query(
      "SELECT id, login, amount, points, credited, package_id FROM donate_orders WHERE id=? FOR UPDATE",
      [externalRef]
    );
    if (!rows.length) {
      await conn.rollback();
      return false;
    }
    const dbOrder = rows[0];
    if (dbOrder.credited) {
      await conn.commit();
      return true;
    }

    if (!orderMatchesDonateOrder(order, dbOrder)) {
      await conn.query("UPDATE donate_orders SET status='review', updated_at=NOW() WHERE id=?", [externalRef]);
      await conn.commit();
      console.error(`Order ${pix.orderId} paga, mas nao corresponde ao pedido ${externalRef}.`);
      return false;
    }

    const [accountUpdate] = await conn.query(
      "UPDATE accounts SET donate = COALESCE(donate, 0) + ? WHERE UPPER(username)=UPPER(?)",
      [dbOrder.points, dbOrder.login]
    );
    if (accountUpdate.affectedRows !== 1)
      throw new Error(`Conta do pedido ${externalRef} nao encontrada para credito.`);

    await conn.query(
      "UPDATE donate_orders SET status='approved', credited=1, order_id=COALESCE(order_id,?), payment_id=COALESCE(payment_id,?), updated_at=NOW() WHERE id=?",
      [pix.orderId || null, pix.paymentId, externalRef]
    );
    await conn.commit();

    // Pacote com itens (ex.: Evento Infernal) -> entrega no guarda-carga via ImportItem.
    // Best-effort apos o commit: se falhar, o cash ja foi creditado; loga p/ reentrega manual.
    try {
      const pkgDef = DONATE_PACKAGES[dbOrder.package_id];
      if (pkgDef && Array.isArray(pkgDef.items) && pkgDef.items.length)
        writeImportItems(dbOrder.login, pkgDef.items);
    } catch (e) {
      console.error(`Itens do pacote ${dbOrder.package_id} NAO entregues para ${dbOrder.login}: ${e.message}`);
    }

    return true;
  } catch (err) {
    await conn.rollback();
    throw err;
  } finally {
    conn.release();
  }
}

async function fetchMpOrder(orderId) {
  const r = await fetch(`${mercadoPago.apiBase}/v1/orders/${orderId}`, {
    headers: { Authorization: `Bearer ${mercadoPago.accessToken}` }
  });
  const data = await r.json().catch(() => ({}));
  return r.ok && data.id ? data : null;
}

// Lista de pacotes (o site pode usar pra exibir os precos oficiais)
app.get("/api/donate/packages", (_req, res) => {
  res.json({
    ok: true,
    packages: Object.entries(DONATE_PACKAGES).map(([id, p]) => ({
      id,
      name: p.name,
      amount: p.amount,
      points: p.points
    }))
  });
});

// Cria a Order Pix (Orders API) e devolve o QR (imagem + copia-e-cola)
app.post("/api/donate/checkout", authRequired, async (req, res) => {
  try {
    if (!mercadoPago.accessToken)
      return res.status(503).json({ ok: false, message: "Pagamento ainda nao configurado. Tente mais tarde." });

    const pkg = DONATE_PACKAGES[String(req.body.packageId || "")];
    if (!pkg) return res.status(400).json({ ok: false, message: "Pacote invalido." });

    const [rows] = await pool.query(
      "SELECT username, email FROM accounts WHERE UPPER(username)=UPPER(?) LIMIT 1",
      [req.login]
    );
    if (!rows.length) return res.status(404).json({ ok: false, message: "Conta nao encontrada." });
    const login = rows[0].username;

    // 1) cria o pedido interno (PENDING) para obter o id = external_reference
    const [ins] = await pool.query(
      "INSERT INTO donate_orders (login, package_id, amount, points, status, credited) VALUES (?,?,?,?, 'pending', 0)",
      [login, String(req.body.packageId), pkg.amount, pkg.points]
    );
    const donateOrderId = ins.insertId;

    // 2) cria a Order Pix no Mercado Pago (Checkout Transparente via Orders)
    const amountStr = pkg.amount.toFixed(2);
    const isTest = mercadoPago.environment === "test";
    const payerEmail = isTest
      ? "test_user_donate@testuser.com"
      : (rows[0].email || `${login.toLowerCase()}@players.wydkingdom.com.br`);
    const payerName = isTest ? "APRO" : login;

    const mpRes = await fetch(`${mercadoPago.apiBase}/v1/orders`, {
      method: "POST",
      headers: {
        Authorization: `Bearer ${mercadoPago.accessToken}`,
        "Content-Type": "application/json",
        "X-Idempotency-Key": crypto.randomUUID()
      },
      body: JSON.stringify({
        type: "online",
        external_reference: String(donateOrderId),
        total_amount: amountStr,
        processing_mode: "automatic",
        payer: { email: payerEmail, first_name: payerName },
        transactions: {
          payments: [
            { amount: amountStr, payment_method: { id: "pix", type: "bank_transfer" } }
          ]
        }
      })
    });
    const data = await mpRes.json().catch(() => ({}));
    if (!mpRes.ok || !data.id) {
      console.error("MP order falhou:", JSON.stringify(data).slice(0, 500));
      await pool.query("UPDATE donate_orders SET status='failed', updated_at=NOW() WHERE id=?", [donateOrderId]);
      return res.status(502).json({ ok: false, message: "Falha ao gerar o Pix. Tente novamente." });
    }

    const pix = extractOrderPix(data);
    await pool.query(
      "UPDATE donate_orders SET order_id=?, payment_id=?, status=?, updated_at=NOW() WHERE id=?",
      [pix.orderId || null, pix.paymentId, pix.status || "pending", donateOrderId]
    );

    return res.json({
      ok: true,
      donateOrderId,
      mercadoPagoOrderId: pix.orderId,
      mercadoPagoPaymentId: pix.paymentId,
      status: pix.status,
      amount: pkg.amount,
      points: pkg.points,
      qrCode: pix.qrCode,
      qrCodeBase64: pix.qrCodeBase64,
      ticketUrl: pix.ticketUrl
    });
  } catch (err) {
    console.error("checkout erro:", err.message);
    return res.status(500).json({ ok: false, message: "Erro ao gerar o pagamento." });
  }
});

// Consulta o status do pagamento (o site faz polling). Se ainda pendente,
// consulta o Mercado Pago ao vivo e credita na hora se ja aprovou.
app.get("/api/donate/status/:donateOrderId", authRequired, async (req, res) => {
  const donateOrderId = String(req.params.donateOrderId);
  if (!/^\d+$/.test(donateOrderId))
    return res.status(400).json({ ok: false, message: "Pedido invalido." });

  const [rows] = await pool.query(
    "SELECT id, order_id, status, credited, points FROM donate_orders WHERE id=? AND UPPER(login)=UPPER(?) LIMIT 1",
    [donateOrderId, req.login]
  );
  if (!rows.length) return res.status(404).json({ ok: false, message: "Pedido nao encontrado." });

  let { status, credited, points } = rows[0];
  const orderId = rows[0].order_id;

  if (!credited && orderId && mercadoPago.accessToken) {
    try {
      const order = await fetchMpOrder(orderId);
      if (order) {
        await creditOrderIfPaid(order);
        const [fresh] = await pool.query(
          "SELECT status, credited, points FROM donate_orders WHERE id=? LIMIT 1",
          [donateOrderId]
        );
        if (fresh.length) ({ status, credited, points } = fresh[0]);
      }
    } catch {
      /* mantem o status do banco */
    }
  }

  res.json({ ok: true, status, credited: !!credited, points });
});

// Webhook do Mercado Pago: re-consulta o pagamento (fonte de verdade) e credita.
app.post("/api/donate/webhook", async (req, res) => {
  try {
    if (!mercadoPago.accessToken) return res.status(200).end();

    const event = parseMercadoPagoWebhook(req);
    // fluxo atual usa Orders -> so tratamos eventos de Order
    if (!event.isOrder || !event.resourceId)
      return res.status(200).end();

    if (!verifyMercadoPagoWebhookSignature(req, mercadoPago.webhookSecret)) {
      console.warn(`Webhook Mercado Pago rejeitado: assinatura invalida (order ${event.resourceId}).`);
      return res.status(401).end();
    }

    const order = await fetchMpOrder(event.resourceId);
    if (order) await creditOrderIfPaid(order);
    return res.status(200).end();
  } catch (err) {
    console.error("webhook erro:", err.message);
    return res.status(200).end();
  }
});

// ---- Erros (ex.: JSON invalido) ----
app.use((err, _req, res, _next) => {
  if (err?.type === "entity.parse.failed")
    return res.status(400).json({ ok: false, message: "Requisicao invalida." });
  if (err?.type === "entity.too.large")
    return res.status(413).json({ ok: false, message: "Imagem muito grande." });
  console.error(err);
  res.status(500).json({ ok: false, message: "Erro interno." });
});

const PORT = Number(process.env.PORT || 80);
const BIND = process.env.BIND || "0.0.0.0";

const startup = process.env.SKIP_DB_MIGRATION === "1"
  ? Promise.resolve()
  : migrate().then(ensureDonateSchema).then(ensurePasswordResetSchema).then(ensureGuildmarkSchema);

startup
  .then(() => {
    console.log(
      `Mercado Pago: env=${mercadoPago.environment}, token=${mercadoPago.accessToken ? "configurado" : "ausente"}, assinatura=${mercadoPago.webhookSecret ? "ativa" : "inativa"}`
    );
    app.listen(PORT, BIND, () => console.log(`WYD Kingdom API ouvindo em ${BIND}:${PORT}`));
  })
  .catch((err) => {
    console.error("Falha ao iniciar (migracao do banco):", err.message);
    process.exit(1);
  });
