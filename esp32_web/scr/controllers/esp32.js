const db = require("../config/firebase")
const axios = require('axios');
const unlock = async (req, res) => {
  try {
  //   res.send("hello");
    await db.ref('/unlock_request').set(true); // Gửi tín hiệu mở khóa
    res.status(200).send('Unlock signal sent');
  // const response = await axios('http://10.45.45.115/display');
  // res.send("ok");
  } catch (err) {
    res.status(500).send('Failed to send unlock signal');
  }
};

const reset = async (req, res) => {
  try {
    await db.ref('/unlock_request').set(false); // Reset sau khi đã mở
    res.status(200).send('Reset done');
  } catch (err) {
    res.status(500).send('Reset failed');
  }
}


module.exports = { unlock, reset }
