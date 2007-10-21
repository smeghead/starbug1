Event.observe(window, 'load', initPage);

function initPage(e) {
    var required_fields = new Array();
    Event.observe('reply_form', 'submit', 
        function(e) {
            try {
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
            } catch (ex) {
                window.status = "javascript error - starbug1 " + ex.toString();
                if (e.preventDefault) {
                    e.preventDefault();
                } else {
                    event.returnValue = false;
                }
            }
        }
    );
    try {
        var i = 1;
        while (true) {
            var field = $('field' + i++);
            if (field == undefined) break;
            var required = $(field.id + '.required');
            if (required == undefined) break;
            if (required.getAttribute('required') == 1) {
                required_fields.push(field);
            }
        }
    } catch (ex) {
        window.status = "javascript error - starbug1 " + ex.toString();
    }
}
function check_input_value(fields) {
    var ret = $A(fields).findAll(
        function(f) {
            var empty = $F(f).empty();
            var elem = $(f.id + '.required');
            if (empty) {
                elem.innerHTML = "必須項目です。入力してください。";
                elem.style.display = "block";
            } else {
                elem.innerHTML = "";
                elem.style.display = "none";
            }
            return empty;
        }
    );
    return (ret.length == 0);
}
