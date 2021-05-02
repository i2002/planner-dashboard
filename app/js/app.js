function httpGetAsync(url, callback, error)
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4) {
            if(xmlHttp.status == 200)
                callback(xmlHttp.responseText);
            else
                error(xmlHttp);
        }   
    }

    xmlHttp.onerror = error;
    xmlHttp.onabort = error;
    xmlHttp.open("GET", url, true);
    xmlHttp.send();
}

function httpPostAsync(url, body, callback, error)
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4) {
            if(xmlHttp.status == 200)
                callback(xmlHttp.responseText);
            else
                error(xmlHttp);
        }   
    }

    xmlHttp.onerror = error;
    xmlHttp.onabort = error;
    xmlHttp.open("POST", url, true);
    xmlHttp.send(body);
}

class App
{
    constructor()
    {
        this.listeners = new Set();
        this.interval = setInterval(() => this.tick(), 500);
    }

    tick()
    {
        this.listeners.forEach(listener => listener.tick());
    }

    register(listener)
    {
        this.listeners.add(listener);
    }

    deregister(listener)
    {
        this.listeners.delete(listener);
    }
}