Event.observe(window, 'load', initPage);

function initPage(e) {
    var links = $$('a.ticket-link');
    links.each(function(elm){
            var icon = document.createElement('div');
            icon.setAttribute('class', 'ticket-link-copy');
            icon.innerHTML = '&nbsp;';
            Event.observe(icon, 'click', function(e){
                var divwin = $('divwin');
                var id = elm.href.match(/(\d+)$/)[1];
                var divwintext = $('divwintext');
                divwintext.innerHTML = '#' + id + ' ' + elm.innerHTML + '\n' + elm.href;
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
            });
            elm.parentNode.appendChild(icon);
    });
//    Event.observe('edit_top_form', 'submit', 
//        function(e) {
//            var dedide = confirm(_("it will registered. are you ok?"));
//            if (!dedide) Event.stop(e);
//            return dedide;
//        }
//    );
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
