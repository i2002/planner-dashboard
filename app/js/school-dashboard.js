class SchoolDashboard
{
    constructor()
    {
        this.parent = append_card(3, "School Dashboard", "fa-chalkboard", "school");
        this.setup_view();
        this.setup_event_listeners();
    }

    setup_view()
    {
        this.parent.innerHTML = "yess";
    }

    setup_event_listeners()
    {
        this.parent.addEventListener('click', () => this.parent.innerHTML = "clicked");
    }
}