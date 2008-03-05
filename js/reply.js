Event.observe(window, 'load', initPage);

function initPage(e) {
    var required_fields = new Array();
    var date_fields = new Array();
    var status_original = $F('field3');
    Event.observe('reply_form', 'submit', 
        function(e) {
            try {
                if ($F('field3') == status_original) {
                    if (!confirm('状態が変更されていませんが、間違いないですか？')) {
                        Event.stop(e);
                        return false;
                    }
                }
                if (!check_input_value(required_fields) ||
                        !validate_datefield(date_fields)) {
                    alert('入力されていない必須項目または不正な値が指定された項目があります。確認してください。');
                    Event.stop(e);
                    return false;
                }
                var dedide = confirm('返信します。よろしいですか？');
                if (!dedide) 
                    Event.stop(e);
                return dedide;
            } catch(ex) {
                alert('エラーが発生しました。');
                Event.stop(e);
            }
        }
    );
    register_required_fields(required_fields, required_field_indexs);
    register_date_fields(date_fields, date_field_indexs);
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
