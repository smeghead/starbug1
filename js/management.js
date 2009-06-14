Event.observe(window, 'load', initPage);

function initPage(e) {
    var required_fields = $$('input.required');
    Event.observe('management_form', 'submit', 
        function(e) {
            try {
                if (required_fields.length && !check_input_value(required_fields)) {
                    alert('入力されていない必須項目があります。確認してください。');
                    Event.stop(e);
                    return false;
                }
                var dedide = confirm('更新します。よろしいですか？');
                if (!dedide)
                    Event.stop(e);
                return dedide;
            } catch(ex) {
                alert('エラーが発生しました。' + ex);
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
            alert(row_template);
            new Insertion.After(row, row_template);
        });
    });
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
