function append_content(column, content)
{
    if(column != 1 && column != 2 && column != 3)
        return;

    document.querySelector(`#${app_id}`).children[column - 1].innerHTML += content;
}

function append_card(column, name, icon, className, app_id = 'app')
{
    if(column != 1 && column != 2 && column != 3)
        return;

    let element = document.createElement('div');
    element.classList.add('card');
    element.classList.add(className);
    element.innerHTML = 
        `<div class="heading">
            <i class="fas fa-fw ${icon}"></i>
            <span>${name}</span>
        </div>
        <div class="content"></div>`;
    document.querySelector(`#${app_id}`).children[column - 1].appendChild(element);

    return element.querySelector('.content');
}