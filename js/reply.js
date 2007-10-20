Event.observe(window, 'load', initPage);

function initPage(e) {
    Event.observe('reply_form', 'submit', 
        function(e) {
            if (!check_input_value(required_fields)) {
                alert('入力されていない必須項目があります。確認してください。');
                if (e.preventDefault) {
                    e.preventDefault();
                } else {
                    event.returnValue = false;
                }
                return false;
            }
            var dedide = confirm('返信します。よろしいですか？');
            if (!dedide) {
                if (e.preventDefault) {
                    e.preventDefault();
                } else {
                    event.returnValue = false;
                }
            }
            return dedide;
        }
    );
    Event.observe('delete_form', 'submit', 
        function(e) {
             return confirm('削除します。一度削除すると戻せません。よろしいですか？');
        }
    );
    var i = 1;
    var required_fields = new Array();
    while (true) {
        var field = $('field' + i++);
        if (field == undefined) break;
        var required = $(field.id + '.required');
        if (required == undefined) break;
        if (required.getAttribute('required') == 1) {
            required_fields.push(field);
        }
    }
}
function check_input_value(fields) {
    var ret = $A(fields).findAll(
        function(f) {
            var elem = $(f.id + '.required');
            if ($F(f).empty()) {
                elem.innerHTML = "必須項目です。入力してください。";
                elem.style.display = "block";
            } else {
                elem.innerHTML = "";
                elem.style.display = "none";
            }
            return !$F(f).empty();
        }
    );
    return (ret.length == 0);
}
