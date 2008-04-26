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
                    message.innerHTML = "必須項目です。入力してください。";
                    message.style.display = "block";
                    f.focus();
                } else {
                    message.innerHTML = "";
                    message.style.display = "none";
                }
                if (empty) {
                    alert('入力されていない必須項目または不正な値が指定された項目があります。確認してください。');
                    Event.stop(e);
                    return false;
                }
                var dedide = confirm('解析します。よろしいですか？');
                if (!dedide)
                    Event.stop(e);
                return dedide;
            } catch(ex) {
                alert('エラーが発生しました。' + ex);
                Event.stop(e);
            }
        }
    );
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
