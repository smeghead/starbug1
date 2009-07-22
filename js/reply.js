Event.observe(window, 'load', initPage);

function initPage(e) {
    var required_fields = new Array();
    var date_fields = new Array();
    var status_original = $F('field3');
    Event.observe('reply_form', 'submit', 
        function(e) {
            try {
                if ($F('field3') == status_original) {
                    if (!confirm(_("you not modify status, are you ok?"))) {
                        Event.stop(e);
                        return false;
                    }
                }
                if (!check_input_value(required_fields) ||
                        !validate_datefield(date_fields)) {
                    alert(_("there are some required columns or invalid columns. please check."));
                    Event.stop(e);
                    return false;
                }
                var dedide = confirm(_("it will replied. are you ok?"));
                if (!dedide) 
                    Event.stop(e);
                return dedide;
            } catch(ex) {
                alert(_("the error occurred.") + ex);
                Event.stop(e);
            }
        }
    );
    register_required_fields(required_fields, required_field_indexs);
    register_date_fields(date_fields, date_field_indexs);
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
