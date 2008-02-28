Event.observe(window, 'load', initPage);

function initPage(e) {
    Event.observe('new_item_form', 'submit', 
        function(e) {
            try {
                if (!checkFields()) {
                    alert('入力されていない必須項目または不正な値が指定された項目があります。確認してください。');
                    Event.stop(e);
                    return false;
                }
                var dedide = confirm('追加します。よろしいですか？');
                if (!dedide)
                    Event.stop(e);
                return dedide;
            } catch(ex) {
                alert('エラーが発生しました。' + ex);
                Event.stop(e);
            }
        });
}
function checkFields() {
    var f = $('field.name');
    if (!f.value) {
        $('field.name.required').innerHTML = '必須項目です。入力してください。';
        f.focus();
        return false;
    }
    return true;
}
