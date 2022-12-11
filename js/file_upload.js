function register_file_fields() {
    var files = document.getElementsByClassName('file-field');
    console.log(files);
    $A(files).each(function(file){
        var dropZone = file.closest('.file-upload');
        dropZone.addEventListener('dragover', function(e) {
            e.stopPropagation();
            e.preventDefault();
            this.classList.add('select');
        }, false);

        dropZone.addEventListener('dragleave', function(e) {
            e.stopPropagation();
            e.preventDefault();
            this.classList.remove('select');
        }, false);

        dropZone.addEventListener('drop', function(e) {
            e.stopPropagation();
            e.preventDefault();
            this.classList.remove('select');
            var files = e.dataTransfer.files;
            file.files = [files[0]];
        }, false);
    });
}
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
