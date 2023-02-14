var PORT = process.env.PORT || 5000;
var express = require("express");
var app = express();
var http = require("http");
var server = http.Server(app);
var WebSocket = require("ws");
var session = require("express-session");
var ws = new WebSocket.Server({ server });
var router = express.Router();
const flash = require("connect-flash");

// const WSc = new WebSocket.Server({ server });

var database = require("./database");

const path = require("path");

app.use(
  session({
    secret: "secret",
    resave: false,
    saveUninitialized: true,
  })
);

app.set("views", path.join(__dirname, "views"));
app.set("view engine", "ejs");

app.use(flash());
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// app.get("/", (req, res) => {
//   res.render("sample_data");
// });
app.get("/", async (req, res) => {
  var query = "SELECT * FROM logs ORDER BY id DESC";

  database.query(query, function (error, data) {
    if (error) {
      throw error;
    } else {
      res.render("sample_data", {
        title: "Node.js MySQL CRUD Application",
        action: "list",
        connected: !!arduinoClient,
        sampleData: data,
        message: req.flash("success"),
      });
    }
  });
});

let anhSang = 0;
let chuyenDong = 0;

let arduinoClient = false;
var client;
var clients = [];

function broadcast(socket, data) {
  for (let i = 0; i < clients.length; i++) {
    if (clients[i] != socket) {
      clients[i].send(data);
    }
  }
}


ws.on("connection", (socket, req) => {
  if (socket.protocol === "arduino") {
    arduinoClient = socket;
    console.log(`[WSc] ${socket.protocol} client is connected`);
  }

  clients.push(socket);

  socket.on("message", function (data) {
    console.log("socket " + data);
    // const obj = JSON.parse('{"name":"John", "age":30, "city":"New York"}');
    var obj = JSON.parse(data);
    anhSang = obj.AS;
    chuyenDong = obj.CD;
    console.log("Chuyen Dong: " + obj.CD);
    console.log("Anh Sang: " + obj.AS);
    console.log("Trang Thai: " + obj.TT);

    console.log(sqlDate);
    console.log(sqlTime);

    var chuoi =
      "Chuyen Dong: " +
      obj.CD +
      " || Anh Sang: " +
      obj.AS +
      " || STATUS: " +
      obj.TT;
    broadcast(socket, chuoi);
  });

  socket.on("close", () => {
    clients = clients.filter((sk) => sk !== socket);
    if (socket.protocol === "arduino") {
      arduinoClient = false;
      console.log(`[WSc] ${socket.protocol} client is disconnected`);
    }
  });
});
//
app.post("/ON", (req, res) => {
  if (!arduinoClient) {
    console.log("[WSc] No arduino client");
    res.redirect("/");
  }
  else {
    arduinoClient.send("ON");
    res.redirect("/");
  }
});

app.post("/OFF", (req, res) => {
  if (!arduinoClient) {
    console.log("[WSc] No arduino client");
    res.redirect("/");
  } else {
    arduinoClient.send("OFF");
    res.redirect("/");
  }
});

app.post("/AUTO", (req, res) => {
  if (!arduinoClient) {
    console.log("[WSc] No arduino client");
    //  req.flash("msg", "! ESP chưa kết nối!");
    //  req.flash("type", "warning");
    res.redirect("/");
  } else {
    arduinoClient.send("AUTO");

    res.redirect("/");
  }
});

let ts = Date.now();

let date_ob = new Date(ts);
let date = date_ob.getDate();
let month = date_ob.getMonth() + 1;
let year = date_ob.getFullYear();
let hours = date_ob.getHours();
let minutes = date_ob.getMinutes();
let seconds = date_ob.getSeconds();

var sqlDate = year + "-" + month + "-" + date;
var sqlTime = hours + ":" + minutes + ":" + seconds;

app.post("/Luu_Data", (req, res) => {
  if (!arduinoClient) {
    console.log("[WSc] No arduino client");
    //  req.flash("msg", "! ESP chưa kết nối!");
    //  req.flash("type", "warning");
    res.redirect("/");
  } else {
    const query = `INSERT INTO logs (chuyenDong,anhSang,Date, Time) VALUES ("${chuyenDong}", "${anhSang}", "${sqlDate}", "${sqlTime}")`;
    database.query(query, function (error, data) {
      if (error) {
        throw error;
      } else {
        res.redirect("/");
      }
    });
  }
});

app.post("/Xoa_Data", (req, res) => {
  if (!arduinoClient) {
    console.log("[WSc] No arduino client");
    //  req.flash("msg", "! ESP chưa kết nối!");
    //  req.flash("type", "warning");
    res.redirect("/");
  } else {
    const query = `Truncate TABLE logs`;
    database.query(query, function (error, data) {
      if (error) {
        throw error;
      } else {
        res.redirect("/");
      }
    });
  }
});

server.listen(PORT, function () {
  console.log("listening on *:" + PORT);
});
