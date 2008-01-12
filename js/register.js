Event.observe(window, 'load', initPage);

function initPage(e) {
    var required_fields = new Array();
    var date_fields = new Array();
    Event.observe('register_form', 'submit', 
        function(e) {
            try {
                if (!check_input_value(required_fields) ||
                        !validate_datefield(date_fields)) {
                    alert('入力されていない必須項目または不正な値が指定された項目があります。確認してください。');
                    Event.stop(e);
                    return false;
                }
                var dedide = confirm('登録します。よろしいですか？');
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
