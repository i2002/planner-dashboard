function append_content(column, className, content = '', app_id = 'app')
{
    if(column != 1 && column != 2 && column != 3)
        return;

    let element = document.createElement('div');
    element.classList.add(className);
    element.innerHTML = content;
    document.querySelector(`#${app_id}`).children[column - 1].appendChild(element);
    return element;
}

function append_card(column, name, icon, className, app_id = 'app')
{
    let content =
    `<div class="heading">
        <i class="fas fa-fw ${icon}"></i>
        <span>${name}</span>
    </div>
    <div class="content"></div>`;
    let element = append_content(column, className, content, app_id);
    if(element == undefined)
        return;

    element.classList.add('card');

    return element.querySelector('.content');
}