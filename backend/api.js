const { exec } = require('child_process'); 
function wait(time)
{
    return new Promise(r => setTimeout(r, time));
}

module.exports = function(app)
{
    app.post('/encrypt/:key', async function(req, res)
    {
        if(req.files)
        {
            const file = req.files.myFile;
            let fileName;
            if(file.name.endsWith('.jpg'))
            {
                fileName = 'input.jpg';
            }
            else 
            {
                fileName = 'input.png';
            }
            file.mv(`../common/${fileName}`, function(err)
            {
                if(err)
                {
                    throw err;
                }
            });
            const command = `./bin/./combine_test.o ../common/${fileName} ../common/output.png ${req.params.key} 0`;
            exec(command, (error, stdout, stderr) => 
            { 
                if (error) 
                { 
                  console.error(`exec error: ${error}`); 
                  return; 
                } 
                if (stderr!= "") 
                console.error(`stderr: ${stderr}`); 
            });
            await wait(25000);
            res.status(201).send({message: 'ok'});
        }
    });

    app.post('/decrypt/:key', async function(req, res)
    {
        if(req.files)
        {
            const imageFile = req.files.myFile;
            const datFile = req.files.datFile;
            let fileName;
            if(imageFile.name.endsWith('.jpg'))
            {
                fileName = 'input.jpg';
            }
            else 
            {
                fileName = 'input.png';
            }
            
            imageFile.mv(`../common/${fileName}`, function(err)
            {
                if(err)
                {
                    throw err;
                }
                else
                {
                    console.log('File uploaded.');
                }
            });

            datFile.mv(`../common/input.dat`, function(err)
            {
                if(err)
                {
                    throw err;
                }
                else
                {
                    console.log('File uploaded.');
                }
            });

            const command = `./bin/./combine_test.o ../common/${fileName} ../common/output.png ${req.params.key} 1 ../common/input.dat`;
            exec(command, (error, stdout, stderr) => 
            { 
                if (error) 
                { 
                  console.error(`exec error: ${error}`); 
                  return; 
                } 
                if (stderr!= "") 
                console.error(`stderr: ${stderr}`); 
            });
            await wait(25000);
            res.status(201).send({message: 'ok'});
        }
    });

    app.delete('/', async function(req, res)
    {
        await wait(60000);
        const command = 'rm ../common/*.*';
        exec(command, (error, stdout, stderr) => 
        { 
            if (error) 
            { 
              console.error(`exec error: ${error}`); 
              return; 
            } 
            if (stderr!= "") 
            console.error(`stderr: ${stderr}`); 
        });
    });
}