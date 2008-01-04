Event.observe(window, 'load', initPage);

function initPage(e) {
    var required_fields = new Array();
    Event.observe('edit_css_form', 'submit', 
        function(e) {
            var dedide = confirm('登録します。よろしいですか？');
            if (!dedide) Event.stop(e);
            return dedide;
        }
    );
}
