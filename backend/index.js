const express = require('express');
const uploader = require('express-fileupload');
const paths = require('./api');
const cors = require('cors');

const app = express();
app.use(uploader());
app.use(cors());
paths(app);

app.listen(4000, () => console.log('Server started.'));