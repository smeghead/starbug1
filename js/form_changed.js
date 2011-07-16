Event.observe(window, 'load', function (e) {
    var changed = false;
    $$('form.changed_check input,form.changed_check select,form.changed_check textarea').each(function(element){
        var elm = element;
        Event.observe(elm, 'change', function(e) {
            changed = true;
        });
    });
    Event.observe(window, 'beforeunload', function(e) {
        if (changed) {
            var message = _("form edited. leave this page?");
            return e.returnValue = message;
        }
    });
});
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
