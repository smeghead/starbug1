Event.observe(window, 'load', initPage);

function initPage(e) {
    var required_fields = $$('input.required');
    Event.observe('management_form', 'submit', 
        function(e) {
            try {
                if (required_fields.length && !check_input_value(required_fields)) {
                    alert(_("there are some required columns. please check."));
                    Event.stop(e);
                    return false;
                }
                var dedide = confirm(_("it will updated. it is ok with you?"));
                if (!dedide)
                    Event.stop(e);
                return dedide;
            } catch(ex) {
                alert(_("the error occurred.") + ex);
                Event.stop(e);
            }
            return true;
        });
    var buttons = $$('input.add_new_row');
    buttons = buttons.constructor == Array ? buttons : [buttons];
    buttons.each(function(b) {
        var no = 1;
        Event.observe(b, 'click', function(e) {
            var field_id = b.id.substring('add_new_row.field'.length);
            var parents = b.ancestors();
            var row = parents[1]; //trオブジェクトを取得する。
            var row_template = $('row_template').innerHTML;
            row_template = row_template.replace(/\[id\]/g, field_id).replace(/\[no\]/g, no++);
            new Insertion.After(row, row_template);
        });
    });
    var checkboxes = $$('input.item-delete-checkbox');
    checkboxes.each(function(c){
        Event.observe(c, 'click', function(e) {
            var element = Event.element(e);
            var rowId = element.id.replace(/field/, 'row').replace(/\.delete.*/, '');
            if (element.checked) {
                $(rowId).addClassName('deleted');
            } else {
                $(rowId).removeClassName('deleted');
            }
        });
    });
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
