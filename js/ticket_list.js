Event.observe(window, 'load', initPage);

function setup_copy_icon(e, text) {
    navigator.clipboard.writeText(text)
        .then(() => {
            var body = $$('body')[0];
            var message = document.createElement('div');
            message.setAttribute('class', 'copied-message');
            message.innerHTML = 'Copied!';
            message.style.top = (Event.pointerY(e) - 40) + 'px';
            message.style.left = (Event.pointerX(e) - 20) + 'px';
            body.appendChild(message);
            setTimeout(function(){
                Element.remove(message);
            }, 1000);
        })
        .catch(err => {
            console.log(err);
            alert('ERROR: Something went wrong');
        });
}
function initPage(e) {
    if ( ! navigator.clipboard) {
        return;
    }
    // create copy icon into ticket list subjects.
    var links = $$('a.ticket-link');
    links.each(function(elm){
        var icon = document.createElement('div');
        icon.setAttribute('class', 'ticket-link-copy');
        icon.innerHTML = '&nbsp;';
        Event.observe(icon, 'click', function(e){
            var id = elm.href.match(/(\d+)$/)[1];
            setup_copy_icon(e, '#' + id + ' ' + elm.innerHTML + '\n' + elm.href);
        });
        elm.parentNode.appendChild(icon);
    });
    // create copy icon into ticket page subject.
    var subject = $('subject');
    if (subject) {
        var icon = document.createElement('span');
        icon.setAttribute('class', 'ticket-page-ticket-link-copy');
        icon.innerHTML = '&nbsp;';
        Event.observe(icon, 'click', function(e){
            var title = $('ticket_title') ? $('ticket_title').innerHTML : '';
            setup_copy_icon(e, title + '\n' + document.location.href);
        });
        subject.appendChild(icon);
    }
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
