class School
{
    constructor(app, column = 1)
    {
        this.parent = append_card(column, "School", "fa-chalkboard", "school");
        this.old_status = -1;
        this.get_data().then(() => {
            this.render_view();
            this.setup_event_listeners();
            app.register(this);
        }).catch(err => console.error(err));
    }

    tick()
    {
        this.render_view();
    }

    render_view()
    {
        let status = this.get_status();
        if(JSON.stringify(status) == JSON.stringify(this.old_status))
        {
            return;
        }
        this.old_status = status;

        if(status == null)
        {
            this.parent.innerHTML = "";
            this.parent.style.display = "none";
            return;
        }

        let subject = status.subject;
        let schedule = this.render_schedule();
        let actions = '';
        subject.links.forEach(link => actions += `<a href="${link.url}" class="button">${link.name}</a>`)

        this.parent.style.display = "";
        this.parent.innerHTML = `<span class="message">${status.message}</span>
        <span class="subject">${subject.name}</span>
        <div class="schedule">${schedule}</div>
        <div class="actions">
            ${actions}
            <span class="button dropdown more" data-action="more">More</span>    
            <span class="button dropdown less" data-action="less">Less</span>
        </div>`;
        
    }

    render_schedule()
    {
        let date = new Date(Date.now());
        let weekday = date.getDay();
        let render = '';
        this.schedule[weekday - 1].forEach(entry => {
            let subject = this.subjects[entry.subject];
            let links = '';
            subject.links.forEach(link => links += `<a href="${link.url}">${link.name}</a>`);
            render += `<li><span>${subject.name}</span>${links}</li>`;
        })
        return `<div class="separator"></div><ul>${render}</ul><div class="separator"></div>`
    }

    get_status()
    {
        let date = new Date(Date.now());
        let minutes = date.getMinutes();
        let hours = date.getHours();
        let time = date.getHours() * 60 + date.getMinutes();
        let weekday = date.getDay();

        if(weekday == 0 || weekday == 6)
            return null;
        
        let subject = -1;
        let message = "";
        this.schedule[weekday - 1].forEach(entry => {
            let start = Math.floor(entry.from / 100) * 60 + entry.from % 100;
            let stop = Math.floor(entry.to / 100) * 60 + entry.to % 100;
            if(time >= start && time <= stop) {
                subject = entry.subject;
                message = "In progress";
            }
            else if(Math.abs(start - time) <= 10)
            {
                subject = entry.subject;
                message = "Up next";
            }
        });

        return subject != -1 ? {subject: this.subjects[subject], message} : null;
    }

    get_data()
    {
        return new Promise((resolve, reject) => {
            httpGetAsync("app:data?action=school-schedule", data => {
                this.schedule = JSON.parse(data);
                httpGetAsync("app:data?action=school-subjects", subjects => {
                    this.subjects = JSON.parse(subjects);
                    resolve();
                }, reject);
            }, reject);
        });
    }

    setup_event_listeners()
    {
        this.parent.addEventListener('click', (e) => {
            if(e.target.tagName == 'SPAN' && (e.target.dataset.action=="more" || e.target.dataset.action=="less")) {
                this.parent.classList.toggle('details');
            }
        });
    }
}