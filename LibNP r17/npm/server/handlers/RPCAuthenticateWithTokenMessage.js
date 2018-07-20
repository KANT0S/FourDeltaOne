var authapi = require('./../authapi');

module.exports = function(data, id)
{
    authapi.authRequest(this, data, id);
};