Event.observe(window, 'load', initPage);

function initPage(e) {
    Event.observe('management_form', 'submit', 
        function(e) {
            return confirm('更新します。よろしいですか？');
        });
}
