const express = require('express');
require('dotenv').config();
const path = require('path');
const morgan = require('morgan');
const { engine } = require('express-handlebars');
const route = require('./routes'); 
const mysql = require('./config/mysqldb');
// const cors = require('cors');

const app = express();
const PORT = 3000;

app.use(express.static(path.join(__dirname, 'public')));
app.use(morgan('combined'));
const cookieParser = require('cookie-parser');  
app.use(cookieParser());
app.engine(
  'hbs',
  engine({
    extname: '.hbs'
  }),
);
app.set('view engine', 'hbs');
app.set('views', path.join(__dirname, 'public', 'views'));
app.use(express.static('public'));
app.use(express.json());
app.use(
    express.urlencoded({
        extended: true,
    }),
);
// app.use(cors());

mysql.connectDB();
route(app);

app.listen(PORT, () => {
  console.log(`Server đang chạy tại http://localhost:${PORT}`);
});
