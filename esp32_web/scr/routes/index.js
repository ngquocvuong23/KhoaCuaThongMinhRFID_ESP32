const authRouter = require('./auth');
const esp32Router = require('./esp32');
const pageRouter = require('./page');

function route(app) {
    app.use('/auth', authRouter);
    app.use('/esp32', esp32Router);
    app.use('/', pageRouter);
}

module.exports = route;
