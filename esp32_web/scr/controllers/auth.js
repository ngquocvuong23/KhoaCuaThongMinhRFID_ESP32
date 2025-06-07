const jwt = require('jsonwebtoken');
const UserModel = require('../models/userModel');

const sendResponse = (res, statusCode, message) => {
    return res.status(statusCode).json({ message });
};

const generateToken = (user) => {
    const payload = { id: user.id, role: user.role, authorized: user.authorized };
    return jwt.sign(
        payload,
        process.env.JWT_SECRET,
        { expiresIn: process.env.JWT_EXPIRES_IN, algorithm: "HS256", }
    );
}

const register = async (req, res) => {
    const { username, password } = req.body;
    try {
        const existingUser = await UserModel.find(username);
        if (existingUser) return sendResponse(res, 400, 'Username đã được sử dụng');

        await UserModel.create(username, password);

        sendResponse(res, 201, "Đăng ký thành công");
    } catch (error) {
        sendResponse(res, 500, 'Lỗi server');
    }
}

const login = async (req, res) => {
    const { username, password } = req.body;
    try {
        const user = await UserModel.find(username);
        if (!user) {
            return sendResponse(res, 400, 'username chưa đăng ký')
        }
        if (password !== user.password) {
            return sendResponse(res, 400, 'Sai mật khẩu');
        }
        const token = generateToken(user);
        res.cookie("token", token, {
            httpOnly: true,
            secure: true,
            sameSite: "Strict",
            maxAge: 24 * 60 * 60 * 1000,
        });

        sendResponse(res, 201, "Đăng nhập thành công");
    } catch (error) {
        sendResponse(res, 500, 'Lỗi server');
    }
}

const authorize = async (req, res) => {
    const { username } = req.body;
    try {
        const user = await UserModel.find(username);
        if (!user) {
            return sendResponse(res, 400, 'username chưa đăng ký')
        }
        if (user.authorized === true) {
            return sendResponse(res, 403, "username đã được cấp quyền");
        }

        await UserModel.update(username, true);
        sendResponse(res, 201, "Đã cấp quyền");
    }
    catch (error) {
        sendResponse(res, 500, 'Lỗi server');
    }
}

module.exports = { register, login, authorize }