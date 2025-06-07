const express = require('express');
const router = express.Router();

router.get('/home', (req, res) => res.render('home'));
router.get('/login', (req, res) => res.render('login'));
router.get('/dashboard', (req, res) => res.render('dashboard'));
router.get('/admin', (req,res) => res.render('/'));
router.get('/hello', (req, res) => res.send("ok"));
router.get('/', (req, res) => res.render('home'));

module.exports = router;