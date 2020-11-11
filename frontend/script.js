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