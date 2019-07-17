class CreateMeasures < ActiveRecord::Migration[5.2]
  def change
    create_table :measures do |t|
      t.float :temp_out
      t.float :temp_in

      t.timestamps
    end
  end
end
