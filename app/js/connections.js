class Connections
{
    constructor(app, column = 1)
    {
        this.parent = append_card(column, "Connections", "fa-ethernet", "connections");
        this.render_view();
        this.setup_event_listeners();
        app.register(this);
    }

    tick()
    {
        this.update_view();
    }

    render_view()
    {
        this.parent.innerHTML = `
        <ul>
            <li class="droidcam" data-status="idle">
                <span>Droidcam</span>
                <i class="fas fa-fw fa-video"></i>
            </li>
            <li class="multimonitor" data-status="unset">
                <span>Multimonitor</span>
                <i class="fas fa-fw fa-desktop"></i>
                <i class="fas fa-fw fa-redo"></i>
            </li>
        </ul>`
    }

    update_view()
    {
        httpGetAsync("app:data?action=droidcam-status", (data) => {
            let dc = this.parent.querySelector(".droidcam");
            dc.dataset.status = data;

            if(data == "idle") {
                dc.querySelector("i").classList.remove("fa-stop");
                dc.querySelector("i").classList.add("fa-video");
            } else if(data == "connecting") {
                // dc.classList.add("connecting");
            } else if(data == "connected") {
                dc.querySelector("i").classList.remove("fa-video");
                dc.querySelector("i").classList.add("fa-stop");
                // dc.classList.remove("connecting");
            }
        });

        httpGetAsync("app:data?action=multimonitor-status", (data) => {
            let dc = this.parent.querySelector(".multimonitor");
            dc.dataset.status = data;
        });
    }

    setup_event_listeners()
    {
        this.parent.querySelector(".droidcam i").addEventListener("click", (e) => {
            if(e.target.parentElement.dataset.status == "idle") {
                httpPostAsync("app:data?action=droidcam-start", "", (data) => this.update_view(), (err) => console.error(err));
            } else if(e.target.parentElement.dataset.status == "connected") {
                httpPostAsync("app:data?action=droidcam-stop", "", (data) => this.update_view(), (err) => console.error(err));
            }
        });

        this.parent.querySelector(".multimonitor i.fa-desktop").addEventListener("click", (e) => {
            let status = e.target.parentElement.dataset.status;
            if(status == "unset") {
                let position = "left";
                httpPostAsync(`app:data?action=multimonitor-setup&position=${position}`, "", (data) => this.update_view(), (err) => console.error(err));
            }
            else {
                httpPostAsync("app:data?action=multimonitor-disable", "", (data) => this.update_view(), (err) => console.error(err));
            }
        });

        this.parent.querySelector(".multimonitor i.fa-redo").addEventListener("click", (e) => {
            let status = e.target.parentElement.dataset.status;
            if(status == "unset") {
                let position = "right";
                httpPostAsync(`app:data?action=multimonitor-setup&position=${position}`, "", (data) => this.update_view(), (err) => console.error(err));
            }
            else {
                httpPostAsync("app:data?action=multimonitor-reset", "", (data) => this.update_view(), (err) => console.error(err));
            }
        });
    }
}