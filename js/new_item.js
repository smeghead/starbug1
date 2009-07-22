Event.observe(window, 'load', initPage);

function initPage(e) {
    Event.observe('new_item_form', 'submit', 
        function(e) {
            try {
                if (!checkFields()) {
                    alert(_("there are some required columns or invalid columns. please check."));
                    Event.stop(e);
                    return false;
                }
                var dedide = confirm(_("it will added. are you ok?"));
                if (!dedide)
                    Event.stop(e);
                return dedide;
            } catch(ex) {
                alert(_("the error occurred.") + ex);
                Event.stop(e);
            }
        });
}
function checkFields() {
    var f = $('field.name');
    if (!f.value) {
        $('field.name.required').innerHTML = _("it will reuqired. please describe.");
        f.focus();
        return false;
    }
    return true;
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
