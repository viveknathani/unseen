const getKey = function()
{
    let result = '';
    const possibleValues = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'];
    for(let i = 0; i < 64; i++)
    {
        let index = Math.floor(Math.random() * (possibleValues.length));
        result += possibleValues[index];
    }
    return result;
}

const getID = function(id) 
{
   return document.getElementById(id);
}
  
const show = function(id) 
{
    getID(id).style.display ='block';
}

const hide = function(id) 
{
    getID(id).style.display ='none';
}

const printKey = function(id)
{
    getID(id).innerText = getKey();
}

const clearIt = function(id)
{
    getID(id).innerText = "";
}

const sendForEncrypt = function()
{
    const key = getID("key").value;
    const image = getID("imageToEncrypt").files[0];

    const formData = new FormData();
    formData.append('myFile', image);

    getID('p_server_en').innerText = "Waiting for server response...";

    fetch(`http://localhost:4000/encrypt/${key}`, { method: 'POST', body: formData })
                                                                                .then(response => response.json())
                                                                                .then(data => {
                                                                                    if(data.message == "ok")
                                                                                    {
                                                                                        getID('p_server_en').innerText = "Success!";
                                                                                        getID('server_en_image').style.display = 'block';
                                                                                        getID('server_en_datfile').style.display = 'block';
                                                                                    }
                                                                                })
                                                                                .catch(error => console.error(error));
}

const sendForDecrypt = function()
{
    const key = getID("key-decrypt").value;
    const image = getID("imageToDecrypt").files[0];
    const datFile = getID("datFile").files[0];

    const formData = new FormData();
    formData.append('myFile', image);
    formData.append('datFile', datFile);

    getID('p_server_de').innerText = "Waiting for server response...";

    fetch(`http://localhost:4000/decrypt/${key}`, { method: 'POST', body: formData })
                                                                                .then(response => response.json())
                                                                                .then(data => {
                                                                                    if(data.message == "ok")
                                                                                    {
                                                                                        getID('p_server_de').innerText = "Success!";
                                                                                        getID('server_de_image').style.display = 'block';
                                                                                        getID('server_de_datfile').style.display = 'block';
                                                                                    }
                                                                                })
                                                                                .catch(error => console.error(error));
}

function wait(time)
{
    return new Promise(r => setTimeout(r, time));
}

const waitAndDelete = function()
{
    fetch(`http://localhost:4000/`, { method: 'DELETE' })
                                                    .then(response => response.json())
                                                    .then(data => console.log(data))
                                                    .catch(error => console.error(error));   
}