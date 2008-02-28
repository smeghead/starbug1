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
}
