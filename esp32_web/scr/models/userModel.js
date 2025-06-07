const { db } = require('../config/mysqldb');

const create = async (username, password) => {
    const query = `INSERT INTO users (username, password) VALUES (?, ?, ?, ?)`;
    return await db.execute(query, [username, password]);
}

const find = async (username) => {
    const query = `SELECT * FROM users WHERE username = ?`;
    const [rows] = await db.execute(query, [username]);
    return rows.length ? rows[0] : null;
}

const update = async (username, authorized) => {
    const query = `UPDATE users SET authorized = ? WHERE username = ?` ;
    return db.execute(query, [authorized, username]);
}

module.exports = { create, find, update };
