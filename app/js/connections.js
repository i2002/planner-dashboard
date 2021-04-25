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
                <i class="fas fa-video"></i>
            </li>
            <li class="multimonitor">
                <span>Multimonitor</span>
                <i class="fas fa-desktop"></i>
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
                dc.classList.add("connecting");
            } else if(data == "connected") {
                dc.querySelector("i").classList.remove("fa-video");
                dc.querySelector("i").classList.add("fa-stop");
                dc.classList.remove("connecting");
            }
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
    }
}