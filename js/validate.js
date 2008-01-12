function register_required_fields(array, ids) {
    $A(ids).each(
        function(i) {if (i > 0) array.push($('field' + i));}
    );
}
function register_date_fields(array, ids) {
    $A(ids).each(
        function(i) {if (i > 0) array.push($('field' + i));}
    );
}

function check_input_value(fields) {
    var ret = $A(fields).findAll(
        function(f) {
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
            return empty;
        }
    );
    return (ret.length == 0);
}
function validate_datefield(fields) {
    var ret = $A(fields).findAll(
        function(f) {
            var not_valid = $F(f).length != 0 && $F(f).match(/^\d{4}-\d{2}-\d{2}$/) != $F(f);
            var message = $(f.id + '.datefield');
            if (not_valid) {
                message.innerHTML = "日付項目です。yyyy-mm-dd形式で入力してください。";
                message.style.display = "block";
                f.focus();
            } else {
                message.innerHTML = "";
                message.style.display = "none";
            }
            return not_valid;
        }
    );
    return (ret.length == 0);
}
