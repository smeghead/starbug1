Event.observe(window, 'load', initPage);

function initPage(e) {
    var required_fields = new Array();
    Event.observe('edit_css_form', 'submit', 
        function(e) {
            var dedide = confirm(_('登録します。よろしいですか？'));
            if (!dedide) Event.stop(e);
            return dedide;
        }
    );
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
