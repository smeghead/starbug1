Event.observe(window, 'load', initPage);

function initPage(e) {
    Event.observe('delete_item_form', 'submit', 
        function(e) {
            var dedide = confirm('削除します。よろしいですか？');
            if (!dedide) Event.stop(e);
            return dedide;
        });
}
