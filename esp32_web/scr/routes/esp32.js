const express = require('express');
const router = express.Router();
const esp32Controller = require('../controllers/esp32');

router.get('/unlock', esp32Controller.unlock);
router.get('/reset', esp32Controller.reset);

module.exports = router;
