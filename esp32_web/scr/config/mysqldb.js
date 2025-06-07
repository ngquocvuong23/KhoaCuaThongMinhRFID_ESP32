const mysql = require('mysql2');

const pool = mysql.createPool({
  connectionLimit: 10, 
  host: process.env.DB_HOST,
  user: process.env.DB_USER,
  password: process.env.DB_PASSWORD,
  database: process.env.DB_NAME
});

const db = pool.promise();

const connectDB = () => {
  db.getConnection()
  .then(connection => {
    console.log('✅ Kết nối MySQL thành công!');
    connection.release(); 
  })
  .catch(err => {
    console.error('❌ Lỗi kết nối MySQL!');
  });
}

module.exports = { db, connectDB};

