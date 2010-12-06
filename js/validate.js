function register_required_fields(array, ids) {
    $A(ids).each(
        function(i) {
            var e = $('field' + i);
            if (e)
                array.push(e);
        }
    );
}
function register_date_fields(array, ids) {
    $A(ids).each(
        function(i) {
            var e = $('field' + i);
            if (e)
                array.push(e);
        }
    );
}
function register_number_fields(array, ids) {
    $A(ids).each(
        function(i) {
            var e = $('field' + i);
            if (e)
                array.push(e);
        }
    );
}

function check_input_value(fields) {
    var ret = $A(fields).findAll(
        function(f) {
            var value = $F(f);
            var new_item = $(f.id + '.new_item');
            var empty = (value.constructor != Array)
                ? value.empty()
                : (value.all(function(x){return x.empty();}) && new_item && $F(new_item).empty());
            if (empty && new_item) {
                /* if new items is appointed, it will be ok. */
                empty = $F(new_item).empty();
            }
            var message = $(f.id + '.required');
            if (empty) {
                message.innerHTML = _("it will reuqired. please describe.");
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
                message.innerHTML = _("it is date column. please describe by yyyy-mm-dd format.");
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
function validate_numberfield(fields) {
    var ret = $A(fields).findAll(
        function(f) {
            var not_valid = $F(f).length != 0 && $F(f).match(/^[+-]?[0-9.]+$/) != $F(f);
            var message = $(f.id + '.numberfield');
            if (not_valid) {
                message.innerHTML = _("it is number column. please describe number.");
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
//最初にフォーカスを当てる。チケット詳細ページで件名にフォーカスが当たると画面がスクロールしてしまうのでコメントアウト。
//Event.observe(window, 'load', function(){
//    var firstControl = $('field1');
//    alert(firstControl);
//    if (firstControl) {firstControl.focus();}
//});
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
