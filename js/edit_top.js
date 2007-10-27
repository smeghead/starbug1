Event.observe(window, 'load', initPage);

function initPage(e) {
    var required_fields = new Array();
    Event.observe('edit_top_form', 'submit', 
        function(e) {
            try {
                var dedide = confirm('登録します。よろしいですか？');
                if (!dedide) {
                    if (e.preventDefault) {
                        e.preventDefault();
                    } else {
                        event.returnValue = false;
                    }
                }
                return dedide;
            } catch (ex) {
                window.status = "javascript error - starbug1 " + ex.toString();
                if (e.preventDefault) {
                    e.preventDefault();
                } else {
                    event.returnValue = false;
                }
            }
        }
    );
}
