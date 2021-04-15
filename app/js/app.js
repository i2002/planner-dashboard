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