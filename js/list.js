Event.observe(window, 'load', initPage);

function toggle(e) {
    if ($('condition_form').style.display == 'none') {
        $('condition_form').style.display = 'block';
        $('display_search_condition').innerHTML = '検索条件を閉じる';
    } else {
        $('condition_form').style.display = 'none';
        $('display_search_condition').innerHTML = '検索条件を開く';
    }
    if (e.preventDefault) {
        e.preventDefault();
    } else {
        event.returnValue = false;
    }
    return false;
}
function initPage(e) {
    Event.observe('display_search_condition', 'click', toggle);
    Event.observe('display_search_condition', 'keydown', toggle);
}
