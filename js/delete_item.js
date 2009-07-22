Event.observe(window, 'load', initPage);

function initPage(e) {
    Event.observe('delete_item_form', 'submit', 
        function(e) {
            var dedide = confirm(_("it will deleted. are you ok?"));
            if (!dedide) Event.stop(e);
            return dedide;
        });
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
