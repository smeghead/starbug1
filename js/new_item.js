Event.observe(window, 'load', initPage);

function initPage(e) {
    Event.observe('new_item_form', 'submit', 
        function(e) {
            return confirm('追加します。よろしいですか？');
        });
}
