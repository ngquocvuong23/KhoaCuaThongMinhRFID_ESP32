const admin = require("firebase-admin");
const serviceAccount = require("./iot-esp32-smartlock-firebase-adminsdk-fbsvc-fdac90ade1.json");

admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: "https://iot-esp32-smartlock-default-rtdb.firebaseio.com"
});

const db = admin.database();
module.exports = db;

