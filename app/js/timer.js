class Timer
{
    constructor(app, column = 2)
    {
        this.parent = append_content(column, 'clock');
        app.register(this);
    }

    tick()
    {
        this.render();
    }

    render()
    {
        // current time
        let d = new Date();

        // time
        let hours = String(d.getHours()).padStart(2, '0');
        let minutes = String(d.getMinutes()).padStart(2, '0')
        let time = `${hours}:${minutes}`;

        // date
        let weekday = new Intl.DateTimeFormat('en', { weekday: 'long' }).format(d);
        let day = Intl.DateTimeFormat('en', { day: '2-digit' }).format(d);
        let month = new Intl.DateTimeFormat('en', { month: 'long' }).format(d);
        let year = Intl.DateTimeFormat('en', { year: 'numeric' }).format(d);
        let date = `${weekday}, ${day} ${month} ${year}`;
        
        // render
        this.parent.innerHTML = `<div class="time">${time}</div><div class="date">${date}</div>`;
    }
}