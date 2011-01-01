Event.observe(window, 'load', initPage);

function setup_copy_icon(e, text) {
    var divwin = $('divwin');
    var divwintext = $('divwintext');
    divwintext.innerHTML = text;
    divwin.style.top = Event.pointerY(e) + 'px';
    divwin.style.left = Event.pointerX(e) + 'px';
    divwin.style.display = 'block';
    divwintext.focus();
    divwintext.select();
    var body = $$('body')[0];
    window.setTimeout(function(){
        Event.observe(body, 'click', function close_divwin(e){
            $('divwin').style.display = 'none';
            Event.stopObserving(body, 'click', close_divwin);
        });
    }, 1000);
}
function initPage(e) {
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
        icon.innerHTML = '&nbsp;&nbsp;&nbsp;';
        Event.observe(icon, 'click', function(e){
            var title = $('ticket_title') ? $('ticket_title').innerHTML : '';
            setup_copy_icon(e, title + '\n' + document.location.href);
        });
        subject.appendChild(icon);
    }
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
