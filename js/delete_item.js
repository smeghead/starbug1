Event.observe(window, 'load', initPage);

function initPage(e) {
    Event.observe('delete_item_form', 'submit', 
        function(e) {
            return confirm('削除します。よろしいですか？');
        });
}
