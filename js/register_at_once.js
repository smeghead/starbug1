Event.observe(window, 'load', initPage);

function initPage(e) {
    var required_fields = new Array();
    var date_fields = new Array();
    Event.observe('register_form', 'submit', 
        function(e) {
            try {
                var f = $('csvdata');
                var empty = $F(f).empty();
                var message = $(f.id + '.required');
                if (empty) {
                    message.innerHTML = _("it will required. please describe.");
                    message.style.display = "block";
                    f.focus();
                } else {
                    message.innerHTML = "";
                    message.style.display = "none";
                }
                if (empty) {
                    alert(_("there are some required columns or invalid columns. please check."));
                    Event.stop(e);
                    return false;
                }
                var dedide = confirm(_("it will analyzed. it is ok with you?"));
                if (!dedide)
                    Event.stop(e);
                return dedide;
            } catch(ex) {
                alert(_("the error occurred.") + ex);
                Event.stop(e);
            }
        }
    );
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
